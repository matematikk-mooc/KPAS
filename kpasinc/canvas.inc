<?php
/*
File   :utility.inc
Author :Erlend Thune
Email  :et@iktsenteret.no
Date   :26.11.2015
Purpose:Utility functions for self registering users in the instructure Canvas platform.
Todo   :Current version tested with feide addresses only.
        Refactor success and error messages.
        Improve error handling. 
*/


function HandleError($result, $url)
{
	echo "<p>$url</p>";
	var_dump($result);
}
function urlify($fields)
{
	//url-ify the data for the POST
	$fields_string = "";
	foreach($fields as $key=>$value) 
	{ 
		$fields_string .= $key.'='.$value.'&'; 
	}
	rtrim($fields_string, '&');
	return $fields_string;
}

function mycurl($url, $fields, $post)
{
    $delete = false;
    return mycurlex($url, $fields, $post, $delete);
}

function mycurldelete($url)
{
    $post = false;
    $delete = true;
	$fields = array();
    return mycurlex($url, $fields, $post, $delete);
}

function mygetcurl($api_endpoint)
{
	global $site;
	global $account_id;
	$url = $site . $api_endpoint;
	mydbg($url);
	$post = 0;
	$delete = 0;
	$fields = array();
	$result = mycurl($url, $fields, $post);
	if (!($result === FALSE)) 
	{
        $content = json_decode($result,true);
        myvardump($content);
        return $content;
    }
    return null;
}

function mypostcurl($api_endpoint, $fields)
{
	global $site;
	global $account_id;
	$url = $site . $api_endpoint;
	mydbg($url);
	$post = 1;
	$result = mycurl($url, $fields, $post);
    return json_decode($result,true);
}

function mycurlex($url, $fields, $post, $delete)
{
	global $verbose;
	global $access_key;
	
	$fields_string = urlify($fields); 
	$curl = curl_init();

	// CURLOPT_VERBOSE: TRUE to output verbose information. Writes output to STDERR, 
	// or the file specified using CURLOPT_STDERR.
	curl_setopt($curl, CURLOPT_VERBOSE, true);

	if($verbose)
	{
		$verbose = fopen('php://temp', 'w+');
		curl_setopt($curl, CURLOPT_STDERR, $verbose);	    
	}

	curl_setopt($curl, CURLOPT_URL, $url); 
	if($post)
	{
		curl_setopt($curl, CURLOPT_POST, count($fields));
		curl_setopt($curl, CURLOPT_POSTFIELDS, $fields_string);
	}
	else if($delete)
	{
    	curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "DELETE");
	}
	curl_setopt($curl, CURLOPT_HTTPHEADER, array('Authorization: Bearer ' . $access_key));
	curl_setopt($curl, CURLOPT_RETURNTRANSFER,TRUE);
	

	mydbg("<p>Executing curl start</p>");

	$result = curl_exec($curl);
	
	if ($result === FALSE) 
	{
		printf("cUrl error (#%d): %s<br>\n", curl_errno($curl),
			   htmlspecialchars(curl_error($curl)));
	}
	mydbg("<p>Curl executed</p>");

	if($verbose)
	{
		rewind($verbose);
		$verboseLog = stream_get_contents($verbose);

		echo "Verbose information:\n<pre>", htmlspecialchars($verboseLog), "</pre>\n";        

		$version = curl_version();
		extract(curl_getinfo($curl));
		$metrics = <<<EOD
URL....: $url
Code...: $http_code ($redirect_count redirect(s) in $redirect_time secs)
Content: $content_type Size: $download_content_length (Own: $size_download) Filetime: $filetime
Time...: $total_time Start @ $starttransfer_time (DNS: $namelookup_time Connect: $connect_time Request: $pretransfer_time)
Speed..: Down: $speed_download (avg.) Up: $speed_upload (avg.)
Curl...: v{$version['version']}
EOD;
	}

	curl_close($curl);
	return $result;
}


//array(6) { ["id"]=> int(1545) ["name"]=> string(22) "test13@erlendthune.com" ["sortable_name"]=> string(22) "test13@erlendthune.com" ["short_name"]=> string(22) "test13@erlendthune.com" ["login_id"]=> string(22) "test13@erlendthune.com" ["locale"]=> NULL } 
//array(5) { ["id"]=> int(1545) ["name"]=> string(22) "test13@erlendthune.com" ["sortable_name"]=> string(22) "test13@erlendthune.com" ["short_name"]=> string(22) "test13@erlendthune.com" ["login_id"]=> string(22) "test13@erlendthune.com" } 


function getUserId($arr)
{
	$user_id = 0;
	foreach($arr as $key=>$value) 
	{
		if($key == "id")
		{
			$user_id = $value;
		}
		//var_dump($key);
		//var_dump($value); 
	}
	return $user_id;
}



function printCanvasUser($canvasUser)
{
    mydbg2("<h2>Canvas user</h2>", 2);
    myvardump2($canvasUser, 2);
}
function printCanvasUserId($user_id)
{
    mydbg2("<br/>Canvas user id: " . $user_id, 2);
}
function getCanvasUserIdFromCanvasUser($canvasUser)
{
    if(!$canvasUser)
    {
        return 0;
    }
    $user_id = $canvasUser[0]["id"];
    return $user_id;
}

function getCanvasCourse($course_id)
{
    global $offline;
    if($offline)
    {
        global $offlineCourse;
        return json_decode($offlineCourse, true);
    }
    $api_endpoint = "/api/v1/courses/" . $course_id;
    $result = mygetcurl($api_endpoint);
    return $result;
}

function getSections($courseId)
{
    $uri = "/api/v1/courses/" . $courseId . "/sections";
    mydbg2($uri, 2);
    $result = mygetcurl($uri);
    myvardump2($result, 2);
    return $result;
}


function getGroups($group_category_id)
{
    $api_endpoint = "/api/v1/group_categories/" . $group_category_id . "/groups?per_page=999";
    $result = mygetcurl($api_endpoint);
    return $result;
}
function AddUserToGroup($userId, $courseId, $groupCategoryId, $jsonGroup)
{
//    echo $jsonGroup;
    $group = json_decode($jsonGroup, true);
//    var_dump($group);
    $groupName = $group["name"];
    $groupDescription = $group["description"];
    $membership = $group["membership"];

    $groupId = createGroup($groupCategoryId, $groupName, $groupDescription);
    $sectionName = $groupName . ":" . $groupDescription;
    $sectionId = createSection($courseId, $sectionName);    
    mydbg2("Section:" . $sectionId, 2);

    $roleId = "StudentEnrollment";
    mydbg2("Membership:".$membership, 2);
    if(($membership == "owner") || ($membership == "admin"))
    {
        $roleId = getRoleIdForPrincipal();
    }
    $enrollment = enrollUserInSection($userId, $roleId, $courseId, $sectionId);
    myvardump2("Enrollment:" . $enrollment,2);

    echo "<br/>Legger bruker med id " . $userId . " til gruppekategori med id " . $groupCategoryId . " i gruppen med id " . $groupId;
    if(AddUserToGroupId($userId, $groupId))
    {
        echo "<br/>Brukeren ble lagt til.";
    }
}

function getRoleIdForPrincipal()
{
    global $account_id;
    global $principalRoleType;
    $roleId = 0;
    $uri = "/api/v1/accounts/" . $account_id. "/roles";
    $roles = mygetcurl($uri);
    myvardump2($roles, 3);
    foreach ($roles as $role)
    {
        if($role["role"] == $principalRoleType)
        {
            myvardump($role,2);
            $roleId = $role["id"];
            break;
        }
    }
    return $roleId;
}

function unenrollUserFromCourse($course_id, $enrollment_id)
{
    $uri = "/api/v1/courses/" . $course_id . "/enrollments/"  . $enrollment_id;
    mycurldelete($uri);
}

function enrollUserInSection($user_id, $role_id, $course_id, $section_id)
{
    $currentEnrollments = getCanvasEnrollments($user_id);
    foreach ($enrollment as $enrollment)
    {
        $enrollment_id = $enrollment["id"];
        unenrollUserFromCourse($course_id, $enrollment_id);
    }
    return enrollUserInSectionEx($user_id, $role_id, $section_id);    
}
function enrollUserInSectionEx($user_id, $role_id, $section_id)
{
	$uri = "/api/v1/sections/" . $section_id . "/enrollments";
	$fields = array( 
		'enrollment[user_id]' => $user_id, 
		'enrollment[role_id]' => $role_id, 
		'enrollment[enrollment_state]' => "active",
		'enrollment[limit_privileges_to_course_section]' => "true",
		'enrollment[self_enrolled]' => 'true'
	); 
	$enrollment = mypostcurl($uri, $fields);
	return $enrollment;
}
function createSection($courseId, $sectionName)
{
    mydbg2("<br/>Undersøker om seksjonen med navnet " . $sectionName . " eksisterer.", 2);
    $sectionId = sectionExist($courseId, $sectionName);
    if($sectionId)
    {
        mydbg2("<br/>Seksjonen eksisterte og har id " . $sectionId, 2);
    }
    else
    {
        mydbg2("<br/>Seksjonen eksisterte ikke. Oppretter den.", 2);
        $createdSection = createSectionEx($courseId, $sectionName);
        myvardump2($createdSection,3);
        $sectionId = $createdSection["id"];
    }
    return $sectionId;
}

function createSectionEx($courseId, $sectionName)
{
    $uri = "/api/v1/courses/" . $courseId . "/sections";
    $fields = array('course_section[name]' => $sectionName);
    $section = mypostcurl($uri, $fields);
    return $section;
}

function createGroupEx($groupCategoryId, $groupName, $groupDescription)
{
  $uri = "/api/v1/group_categories/" . $groupCategoryId . "/groups"; 
  $fields = array( 
    'name' => $groupName, 
    'description' => $groupDescription
  ); 
  
  $newGroup = mypostcurl($uri, $fields);
  return $newGroup;
}
function createGroup($groupCategoryId, $groupName, $groupDescription)
{
    echo "<br/>Undersøker om gruppen med navnet " . $groupName . " og beskrivelsen " . $groupDescription . " eksisterer.";
    $groupId = groupExist($groupName, $groupDescription, $groupCategoryId);
    if($groupId)
    {
        echo "<br/>Gruppen eksisterte og har id " . $groupId;
    }
    else
    {
        echo "<br/>Gruppen eksisterte ikke. Oppretter den.";
        $createdGroup = createGroupEx($groupCategoryId, $groupName, $groupDescription);
        myvardump($createdGroup);
        $groupId = $createdGroup["id"];
    }
    return $groupId;
}


function sectionExist($courseId, $sectionName)
{
    $sectionId = 0;
    $sections = getSections($courseId);
    foreach ($sections as $section)
    {
        mydbg2($section["name"] . "=?" . $sectionName, 2);
        if(($section["name"] == $sectionName))
        {
            $sectionId = $section["id"];
            break;
        }
    }
    return $sectionId;
}

function groupExist($groupName, $groupDescription, $groupCategoryId)
{
    $groupId = 0;
    $groups = getGroups($groupCategoryId);
    foreach ($groups as $group)
    {
        if(($group["name"] == $groupName) && ($group["description"] == $groupDescription))
        {
            $groupId = $group["id"];
            break;
        }
    }
    return $groupId;
}

function getUserGroups($user_id)
{
    global $offline;
    if($offline)
    {
        global $offlineMyGroups;
        myvardump($offlineMyGroups);
        return json_decode($offlineMyGroups,true);
    }
    $api_endpoint = "/api/v1/users/self/groups?as_user_id=" . $user_id . "&per_page=999";
    $result = mygetcurl($api_endpoint);
    return $result;
}

function getGroupCategories($course_id)
{
    global $offline;
    if($offline)
    {
        global $offlineGroupCategories;
        return json_decode($offlineGroupCategories,true);
    }
    $api_endpoint = "/api/v1/courses/" . $course_id . "/group_categories?per_page=999";
    $result = mygetcurl($api_endpoint);
    return $result;
}

function getCanvasEnrollments($user_id)
{
    global $offline;
    
    if(!$user_id)
    {
        return;
    }
    if($offline)
    {
        global $offlineCourseEnrollments;
        return json_decode($offlineCourseEnrollments,true);
    }
    $api_endpoint = "/api/v1/users/" .$user_id . "/enrollments?per_page=999";
    $result = mygetcurl($api_endpoint);
    return $result;
}

function printCanvasEnrollments($canvasEnrollments)
{
    global $verbose;
    if(!$verbose) return;
    echo "<h2>Canvas enrollments</h2>";
    myvardump($canvasEnrollments);
}


function getCanvasUserFromFeideId($feideid)
{
    $api_endpoint = "/api/v1/accounts/self/users?search_term=" . $feideid ;
    mydbg2("getCanvasUserFromFeideId: " . $api_endpoint,3);
    $result = mygetcurl($api_endpoint);
    return $result;
}

function userExist($epost)
{
	global $site;
	global $account_id;
	$url = $site . "/api/v1/users/sis_user_id:" . $epost;
	$user_id = 0;
	
	mydbg("Searching for ". $epost);
	mydbg($url);

	$post = 0;
	$fields = array();
	$result = mycurl($url, $fields, $post);
	if (!($result === FALSE)) 
	{
		mydbg("Call succeeded");
		$content = json_decode($result,true);
		//var_dump($content);

		foreach($content as $key=>$value) 
		{
			if($key == "errors")
			{
				mydbg("User did not exist.");
				break;
			}
			else 
			{
				mydbg("Key:");
				myvardump($key);
				mydbg("Value:");
				myvardump($value); 
				mydbg("Type:" . gettype($key));
				if($key == "id")
				{
					myvardump($value);
					if(sizeof($value))
					{
						mydbg("User exists");	
						$user_id = $value;
						mydbg("user id=" . $user_id);
						break;
					}
				}
				else
				{
					mydbg("User did not exist.");
				}
			}
		}
	}
	else
	{
		mydbg("Call failed");
	}
	return $user_id;
}



function addUserToGroupId($user_id,$gid)
{
	global $site;
	global $verbose;
	$ret_value = 0;

	$url = $site . "/api/v1/groups/" . $gid . "/memberships";
	$fields = array( 
		user_id => $user_id
	); 
	
	$post = 1;
	$result = mycurl($url, $fields, $post);
	if (!($result === FALSE)) 
	{
		$content = json_decode($result,true);

		foreach($content as $key=>$value) 
		{
			mydbg(gettype($key));
			mydbg(gettype($value));
			if($key == "errors")
			{
			    $ret_value = 0;
			    myerror("User:" . $user_id);
				HandleError($result, $url);
			}
			else 
			{
				$ret_value = 1;
//				var_dump($key);
//				var_dump($value); 
			}
		}
	}
	return $ret_value;
}


?>