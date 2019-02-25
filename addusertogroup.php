<?php
require_once 'kpasinc/dataporten.inc';
require_once 'kpasinc/vars.inc';
require_once 'kpasinc/curlutility.inc';
require_once 'kpasinc/canvas.inc';
require_once 'kpasinc/utility.inc';
require_once 'kpasinc/kpasapi.inc';

$result = "";
$status_message = "Success";
$token = "";
$jsonGroup = "";

try {
    if(!isset($_SERVER['HTTP_X_DATAPORTEN_TOKEN'])) {
        throw new Exception("Dataporten token not specified.");
    }
    $token = $_SERVER['HTTP_X_DATAPORTEN_TOKEN'];

    if(!isset($_POST["group"])) {
        throw new Exception("Group parameter not specified.");
    }

    $jsonGroup = $_POST["group"];
    $group = json_decode($jsonGroup, true);
    if(!$group) {
        throw new Exception("Group parameter has invalid syntax.");
    }

    //First get Feide ID from dataporten.
    $dataportenUserInfo = KPASAPI_GetUserInfo();
    if($dataportenUserInfo["error"])
    {
        $errorMessage = $dataportenUserInfo["error"] . " " . $dataportenUserInfo["message"];
        throw new Exception($errorMessage);
    }

    //Now search for that Feide ID in Canvas.
    $feideid = getFeideIdFromDataportenUserInfo($dataportenUserInfo);
    $canvasUser = getCanvasUserFromFeideId($feideid);
    if($canvasUser.errors && $canvasUser.errors.message) {
        $errorMessage = $canvasUser.status . " " . $canvasUser.errors.message[0];
        throw new Exception($errorMessage);
    } 

    //Check that we found the Canvas user.
    $user_id = getCanvasUserIdFromCanvasUser($canvasUser);
    if(!$user_id) {
        throw new Exception("Kunne ikke finne brukeren " . $feideid . " i Canvas.");
    }
    
    //We can add the user to the specified group.
    $result = AddUserToGroup($group);

} catch (Exception $e) {
    $result = $e->getMessage();
    $status_message = "Failure";
}

KPASAPI_Response(200,$status_message,$result);
?>