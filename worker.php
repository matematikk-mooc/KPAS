<?php
session_start();
//require_once 'vendor/autoload.php';
require_once 'kpasinc/dataporten.inc';
require_once 'kpasinc/vars.inc';
require_once 'kpasinc/canvas.inc';
require_once 'kpasinc/utility.inc';

printHtmlHeader();
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $group = $_POST["group"];
    $groupCategoryId = $_POST["groupCategoryId"];
    $userId = $_POST["userId"];
    $courseId = $_POST["courseId"];
    echo "Melder deg inn i " . $group;
    echo "<br/>GruppekategoriId: ". $groupCategoryId;
    echo "<br/>BrukerId: " . $userId;
    AddUserToGroup($userId, $courseId, $groupCategoryId, $group);
}
printDataportenHeading();
$courseId = $_SESSION["courseId"];
printCourseId($courseId);
$userInfo = $_SESSION["userInfo"];
$dataportenGroups = $_SESSION["groups"];
$extraUserInfo = $_SESSION["extraUserInfo"];
printDataportenUserInfo($userInfo);
printDataportenExtraUserInfo($extraUserInfo);
printDataportenGroupsInfo($dataportenGroups);
printCanvasHeading();
$feideid = getFeideIdFromDataportenUserInfo($userInfo);
mydbg2("FeideId: " . $feideid,3);
$canvasUser = null;
if($offline)
{
    $canvasUser = json_decode($offlineCanvasUser,true);
}
else
{
    $canvasUser = getCanvasUserFromFeideId($feideid);
}
printCanvasUser($canvasUser);
$user_id = getCanvasUserIdFromCanvasUser($canvasUser);
printCanvasUserId($user_id);
//$canvasEnrollments = getCanvasEnrollments($user_id);
//printCanvasEnrollments($canvasEnrollments);
//printCanvasEnrollmentGroups($user_id, $canvasEnrollments, $dataportenGroups);
printGroupsForCourse($user_id, $courseId, $dataportenGroups);
//    printCommonGroups($dataportenGroups, $canvasEnrollmentGroups);
printHtmlFooter();

?>