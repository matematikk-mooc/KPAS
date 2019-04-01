<?php
session_start();
//require_once 'vendor/autoload.php';
require_once 'kpasinc/dataporten.inc';
require_once 'kpasinc/vars.inc';
require_once 'kpasinc/curlutility.inc';
require_once 'kpasinc/canvas.inc';
require_once 'kpasinc/utility.inc';

printHtmlHeader();
try {

    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $jsonGroup = $_POST["group"];
        $group = json_decode($jsonGroup, true);
        $result = AddUserToGroup($group["user_id"], $group);
        echo $result;
    }
    $courseId = $_SESSION["courseId"];
    printCourseId($courseId);
    $userInfo = $_SESSION["userInfo"];
    $dataportenGroups = $_SESSION["groups"];
    $extraUserInfo = $_SESSION["extraUserInfo"];
    $feideid = getFeideIdFromDataportenUserInfo($userInfo);
    mydbg("FeideId: " . $feideid);
    $canvasUser = null;
    $canvasUser = getCanvasUserFromFeideId($feideid);

    if(!$canvasUser) {
        throw new Exception("Kunne ikke finne brukeren " . $feideid . " i Canvas.");
    }
    printCanvasUser($canvasUser);
    printDataportenUserInfo($userInfo);
    printDataportenExtraUserInfo($extraUserInfo);
    $user_id = $canvasUser["id"];
    printCanvasUserId($user_id);
    printDataportenGroupsInfo($dataportenGroups);
    printGroupsForCourse($courseId, $dataportenGroups);
    printLogoutButton();
} catch (Exception $e) {
    $result = $e->getMessage();
    print $result;
}
printHtmlFooter();

?>