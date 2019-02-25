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
    global $site;
    global $canvas_access_key;
    if($offline)
    {
        global $offlineCourse;
        return json_decode($offlineCourse, true);
    }
    $api_endpoint = $site . "/api/v1/courses/" . $course_id;
    $result = mygetcurl($api_endpoint, $canvas_access_key);
    return $result;
}

function getSections($courseId)
{
    global $site;
    global $canvas_access_key;
    $uri = $site ."/api/v1/courses/" . $courseId . "/sections";
    mydbg2($uri, 2);
    $result = mygetcurl($uri, $canvas_access_key);
    myvardump2($result, 2);
    return $result;
}


function getGroups($group_category_id)
{
    global $site;
    global $canvas_access_key;

    $api_endpoint = $site . "/api/v1/group_categories/" . $group_category_id . "/groups?per_page=999";
    $result = mygetcurl($api_endpoint, $canvas_access_key);
    return $result;
}
function AddUserToGroup($group)
{
//    echo $jsonGroup;
//    var_dump($group);
    $groupName = $group["name"];
    $groupDescription = $group["description"];
    $membership = $group["membership"];
    $groupCategoryId = $group["group_category_id"];
    $userId = $group["user_id"];
    $courseId = $group["course_id"];

    $groupId = createGroup($groupCategoryId, $groupName, $groupDescription);
    $sectionName = $groupName . ":" . $groupDescription;
    $sectionId = createSection($courseId, $sectionName);    
    mydbg2("Section:" . $sectionId, 2);

    mydbg2("Membership:".$membership, 2);

    mydbg2("<br/>Fjerner tidligere medlemskap.",2);
    unenrollUserFromCourse($userId, $courseId);

    mydbg2("<br/>Legger deg til som student.",2);
    $enrollment = enrollStudentInSection($userId, $courseId, $sectionId);

    if(($membership == "owner") || ($membership == "admin"))
    {
        mydbg2("<br/>Du er gruppeeier eller -admin og får tilgang til skoleledermeny for din gruppe.",2);
        $enrollment = enrollPrincipalInSection($userId, $courseId, $sectionId);
        mydbg2("Studentenrollment:" . $enrollment,2);
    }

    mydbg2("<br/>Legger bruker med id " . $userId . " til gruppekategori med id " . $groupCategoryId . " i gruppen med id " . $groupId,2);
    if(AddUserToGroupId($userId, $groupId))
    {
        mydbg2("<br/>Brukeren ble lagt til.",2);
        return "Success";
    }
    return "Failure";
}

function getRoleIdFor($roleType)
{
    global $account_id;
    global $site;
    global $canvas_access_key;
    
    $roleId = 0;
    $uri = $site . "/api/v1/accounts/" . $account_id. "/roles";
    $roles = mygetcurl($uri, $canvas_access_key);
    myvardump2($roles, 3);
    foreach ($roles as $role)
    {
        if($role["role"] == $roleType)
        {
            myvardump2($role,3);
            $roleId = $role["id"];
            break;
        }
    }
    return $roleId;
}

function unenrollUserFromCourse($user_id, $course_id)
{
    global $site;
    global $canvas_access_key;
    $currentEnrollments = getCanvasEnrollments($user_id);
    foreach ($currentEnrollments as $enrollment)
    {
        if($enrollment["course_id"] == $course_id) {
            $enrollment_id = $enrollment["id"];
            mydbg2("<br/>Fjerner deg fra tidligere seksjon med enrollment id " . $enrollment_id,2);
            $uri = $site . "/api/v1/courses/" . $course_id . "/enrollments/"  . $enrollment_id . "?task=delete";
            mycurldelete($uri, $canvas_access_key);
        }
    }
}

function enrollPrincipalInSection($user_id, $course_id, $section_id)
{
    global $principalRoleType;
    $enrollment = null;
    $role_id = getRoleIdFor($principalRoleType);
    if($role_id)
    {
        mydbg2("Legger deg til i seksjon " . $section_id . " som skoleleder.",2);
        $enrollment = enrollInSection($user_id, $role_id, $course_id, $section_id);
    }
    else
    {
        mydbg2("Kunne ikke finne rolle-id for skoleleder.",2);
    }
    return $enrollment;
}

function enrollInSection($user_id, $role_id, $course_id, $section_id)
{
    global $site;
    global $canvas_access_key;
    $uri = $site . "/api/v1/sections/" . $section_id . "/enrollments";
    $fields = array( 
        'enrollment[user_id]' => $user_id, 
        'enrollment[role_id]' => $role_id, 
        'enrollment[enrollment_state]' => "active",
        'enrollment[limit_privileges_to_course_section]' => "true",
        'enrollment[self_enrolled]' => 'true'
    ); 
    $enrollment = mypostcurl($uri, $fields, $canvas_access_key);
	return $enrollment;
}

function enrollStudentInSection($user_id, $course_id, $section_id)
{
    $roleType = "StudentEnrollment";
    $enrollment = null;
    $role_id = getRoleIdFor($roleType);
    if($role_id)
    {
        mydbg2("Legger deg til i seksjon " . $section_id . " som student.",2);
	    $enrollment = enrollInSection($user_id, $role_id, $course_id, $section_id);
	}
	else
	{
	    mydbg2("Kunne ikke finne rolle-id for student.",2);
	}
	return $enrollment;
}

function createSection($courseId, $sectionName)
{
    mydbg2("<br/>Undersøker om seksjonen med navnet " . $sectionName . " eksisterer.",2);
    $sectionId = sectionExist($courseId, $sectionName);
    if($sectionId)
    {
        mydbg2("<br/>Seksjonen eksisterte og har id " . $sectionId,2);
    }
    else
    {
        mydbg2("<br/>Seksjonen eksisterte ikke. Oppretter den.",2);
        $createdSection = createSectionEx($courseId, $sectionName);
        myvardump2($createdSection,3);
        $sectionId = $createdSection["id"];
    }
    return $sectionId;
}

function createSectionEx($courseId, $sectionName)
{
    global $site;
    global $canvas_access_key;

    $uri = $site . "/api/v1/courses/" . $courseId . "/sections";
    $fields = array('course_section[name]' => $sectionName);
    $section = mypostcurl($uri, $fields, $canvas_access_key);
    return $section;
}

function createGroupEx($groupCategoryId, $groupName, $groupDescription)
{
  global $site;
  global $canvas_access_key;

  $uri = $site . "/api/v1/group_categories/" . $groupCategoryId . "/groups"; 
  $fields = array( 
    'name' => $groupName, 
    'description' => $groupDescription
  ); 
  
  $newGroup = mypostcurl($uri, $fields, $canvas_access_key);
  return $newGroup;
}
function createGroup($groupCategoryId, $groupName, $groupDescription)
{
    mydbg2("<br/>Undersøker om gruppen med navnet " . $groupName . " og beskrivelsen " . $groupDescription . " eksisterer.",2);
    $groupId = groupExist($groupName, $groupDescription, $groupCategoryId);
    if($groupId)
    {
        mydbg2("<br/>Gruppen eksisterte og har id " . $groupId,2);
    }
    else
    {
        mydbg2("<br/>Gruppen eksisterte ikke. Oppretter den.",2);
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
	global $site;
    global $canvas_access_key;
    if($offline)
    {
        global $offlineMyGroups;
        myvardump($offlineMyGroups);
        return json_decode($offlineMyGroups,true);
    }
    $api_endpoint = $site . "/api/v1/users/self/groups?as_user_id=" . $user_id . "&per_page=999";
    $result = mygetcurl($api_endpoint, $canvas_access_key);
    return $result;
}

function getGroupCategories($course_id)
{
    global $offline;
	global $site;
    global $canvas_access_key;
    if($offline)
    {
        global $offlineGroupCategories;
        return json_decode($offlineGroupCategories,true);
    }
    $api_endpoint = $site . "/api/v1/courses/" . $course_id . "/group_categories?per_page=999";
    $result = mygetcurl($api_endpoint, $canvas_access_key);
    return $result;
}

function getCanvasEnrollments($user_id)
{
    global $offline;
    global $site;
    global $canvas_access_key;
    
    if(!$user_id)
    {
        return;
    }
    if($offline)
    {
        global $offlineCourseEnrollments;
        return json_decode($offlineCourseEnrollments,true);
    }
    $api_endpoint = $site . "/api/v1/users/" .$user_id . "/enrollments?per_page=999";
    $result = mygetcurl($api_endpoint, $canvas_access_key);
    return $result;
}

function printCanvasEnrollments($canvasEnrollments)
{
    mydbg("<h2>Canvas enrollments</h2>");
    myvardump($canvasEnrollments);
}


function getCanvasUserFromFeideId($feideid)
{
    global $site;
    global $canvas_access_key;
    
    $api_endpoint = $site . "/api/v1/accounts/self/users?search_term=" . $feideid ;
    mydbg2("getCanvasUserFromFeideId: " . $api_endpoint,2);
    $result = mygetcurl($api_endpoint, $canvas_access_key);
    myvardump2($result,2);
    return $result;
}

function addUserToGroupId($user_id,$gid)
{
    global $site;
    global $canvas_access_key;
	$uri = $site . "/api/v1/groups/" . $gid . "/memberships";
	$fields = array( 
		'user_id' => $user_id
	); 
	return mypostcurl($uri, $fields, $canvas_access_key);
}
?>