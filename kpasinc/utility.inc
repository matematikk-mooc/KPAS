<?php
function mydbg($s)
{
    mydbg2($s,1);
}
function myerror($s)
{
	$stuff = <<<EOT
  <div class="card">
    <div class="card-body bg-danger text-white">$s</div>
  </div>
EOT;
    echo $stuff;
}
function mydbg2($s, $level)
{
	global $verbose2;
	global $verbose_level;

	if($verbose2 && ($verbose_level >= $level))
	{
		error_log($s);
	}
}


function myjson($s)
{
    $json = json_encode($s, JSON_PRETTY_PRINT); 
    error_log($json);
}

function myvardump($v)
{
    myvardump2($v, 1);
}
function myvardump2($v, $level)
{
	global $verbose2;
	global $verbose_level;

	if($verbose2 && ($verbose_level >= $level))
	{
        ob_start();
        var_dump($v);
        $debug_dump = ob_get_clean();
        error_log($debug_dump);
	}
}

function printGroupMemberships($user_id, $course_id, $groupCategories, $myGroups, $dataportenGroups)
{
    global $verbose;
    if($verbose)
    {
        echo "<h3>Gruppekategorier</h3>";
        myvardump($groupCategories);
        echo "<p></p>";
        myvardump($myGroups);
    }
    echo "<table class='table'><tr><th>Gruppekategori</th><th>Nåværende gruppe</th><th>Bli med i gruppe</th></tr>";
    foreach ($groupCategories as $groupCategory)
    {
        echo "<tr><td>" . $groupCategory["name"];
        $groupName = "-";
        $groupDescription = "";
        echo "</td>";
        echo "<td>";
        foreach ($myGroups as $myGroup)
        {
            if($myGroup["group_category_id"] == $groupCategory["id"])
            {
                $groupName = $myGroup["name"];
                $groupDescription = $myGroup["description"];
            }
        }
        echo $groupName . "</br>" . $groupDescription;
        echo "</td>";
        $dp = "<form method='post' action='worker.php'>  <div class='form-group'><select name='group'>";
        foreach ($dataportenGroups as $dataportenGroup)
        {
            $membership = $dataportenGroup["membership"]["basic"];
            $value = "{
            \"name\":\"" . $dataportenGroup["displayName"] . "\",
            \"description\":\"" . $dataportenGroup["id"] . "\",
            \"membership\":\"" . $membership . "\",
            \"group_category_id\":\"" . $groupCategory["id"] . "\",
            \"course_id\":\"" . $course_id . "\",
            \"user_id\":\"" . $user_id . "\"
            }";
            $dp .= "<option value='" . $value .  "'>" . $dataportenGroup["displayName"] . "(" . $membership . ")</option>";
        }
        $dp .= "</select></div><div class='form-group'><button class='btn btn-primary'>Bli med</button></div>";
        $dp .= "</form>";
        echo "<td>" . $dp . "</td></tr>";
    }
    echo "</table>";
}
function printCourseId($courseId)
{
    global $verbose;
    if(!$verbose) return;
    echo "<h1>Course id</h1>" . $courseId . "<br/>";
}

function printGroupsForCourse($user_id, $course_id, $dataportenGroups)
{
    if(!$user_id)
    {
        global $site;
        myerror("Din bruker er ikke registrert på " . $site . "<br/>Registrer deg som bruker der og prøv så igjen.");
        return;
    }
    
    mydbg("Henter Canvaskurs " . $course_id);
    $course = getCanvasCourse($course_id);
    mydbg2("<h3>" . $course["name"] . "</h3>",2);
    $groupCategories = getGroupCategories($course_id);
    $myGroups = getUserGroups($user_id);
    printGroupMemberships($user_id, $course_id, $groupCategories, $myGroups, $dataportenGroups);
}

function printCanvasEnrollmentGroups($user_id, $canvasEnrollments, $dataportenGroups)
{
    global $verbose;
    if($verbose)
    {
        echo "<h2>Canvas enrollment groups</h2>";
        if(!$canvasEnrollments)
        {
            echo "No enrollments.";
            return;
        }
        myvardump($canvasEnrollments);
    }
    
    foreach ($canvasEnrollments as $course)
    {    
        printGroupsForCourse($user_id, $course_id, $dataportenGroups);
    }
}

function printCommonGroups($dataportenGroups, $canvasEnrollmentGroups)
{
    echo "<h1>Common dataporten and Canvas groups</h1>";
    foreach ($canvasEnrollmentGroups as $canvasEnrollmentGroup)
    {
        //For each enrollment
        foreach ($canvasEnrollmentGroup as $canvasGroup)
        {
            foreach ($dataportenGroups as $dataportenGroup)
            {
                if($dataportenGroup["displayName"] == $canvasGroup["name"])
                {
                    $groupName = $dataportenGroup["displayName"];
                    echo "<p>$groupName</p>";
                }
            }
        }
    }
}
function printDataportenHeading()
{
    global $verbose;
    if(!$verbose) return;
    echo "<h1>Dataporten</h1>";
}

function printCanvasHeading()
{
    global $verbose;
    if(!$verbose) return;
    echo "<h1>Canvas</h1>";
}

function printHtmlHeader()
{
	$stuff = <<<EOT
<!DOCTYPE html>
<html lang="en">
<head>
  <title>KPAS (Kompetanseplattform Administrativt System</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.2.1/css/bootstrap.min.css">
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.6/umd/popper.min.js"></script>
  <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.2.1/js/bootstrap.min.js"></script>
</head>
<body>

<div class="container">
EOT;
	echo $stuff;
}

function printLoginButton() 
{
	$stuff = <<<EOT
<div id="button">
	<a id="login" href="index.php?login">Login</a>
</div>
EOT;
    print $stuff;
}

function printLogoutButton()
{
    $stuff = <<<EOT
    <a id="logout" href="cb.php?logout">Logout</a>
EOT;
    print $stuff;
}

function printHtmlFooter()
{
	$stuff = <<<EOT
    </div>
    </body>
    </html>
EOT;
	echo $stuff;
}
?>