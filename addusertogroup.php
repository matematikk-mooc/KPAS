<?php
require_once 'kpasinc/dataporten.inc';
require_once 'kpasinc/vars.inc';
require_once 'kpasinc/curlutility.inc';
require_once 'kpasinc/canvas.inc';
require_once 'kpasinc/utility.inc';
require_once 'kpasinc/kpasapi.inc';

$token = $_SERVER['HTTP_X_DATAPORTEN_TOKEN'];
$jsonGroup = $_POST["group"];
//error_log($group);
$group = json_decode($jsonGroup, true);

$dataportenUserInfo = KPASAPI_GetUserInfo();
$feideid = getFeideIdFromDataportenUserInfo($dataportenUserInfo);
$canvasUser = getCanvasUserFromFeideId($feideid);
$user_id = getCanvasUserIdFromCanvasUser($canvasUser);
$result = AddUserToGroup($group);
KPASAPI_Response(200,"result",$result);
?>