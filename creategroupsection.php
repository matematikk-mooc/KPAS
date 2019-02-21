<?php
require_once 'kpasinc/dataporten.inc';
require_once 'kpasinc/vars.inc';
require_once 'kpasinc/curlutility.inc';
require_once 'kpasinc/canvas.inc';
require_once 'kpasinc/utility.inc';
//header("Content-Type:application/json");

$token = $_SERVER['HTTP_X_DATAPORTEN_TOKEN'];
//$dataporten_clientid = $_SERVER['X-dataporten-clientid'];
$jsonGroup = $_POST["group"];
error_log($group);
$group = json_decode($jsonGroup, true);

function htmlResponse($s)
{
    echo "<html><body>";
    var_dump($_SERVER);
    $headers = apache_request_headers();
    var_dump($headers);

    echo "</body></html>";
}

function KPASGetUserInfo() {
    global $token;
//    response(200,"result",$token);     
//    exit;
    $user_info_url = "https://auth.dataporten.no/userinfo";
    $result = mygetcurl($user_info_url, $token);

    return $result;
}


function response($status,$status_message,$data)
{
	header("HTTP/1.1 ".$status);
	
	$response['status']=$status;
	$response['status_message']=$status_message;
	$response['data']=$data;
	
	$json_response = json_encode($response);
	echo $json_response;
}
$dataportenUserInfo = KPASGetUserInfo();
$feideid = getFeideIdFromDataportenUserInfo($dataportenUserInfo);
$canvasUser = getCanvasUserFromFeideId($feideid);
$user_id = getCanvasUserIdFromCanvasUser($canvasUser);
$group_user_id = $group["user_id"];
$result = "Canvas user id " . $user_id . " does not match " . $group_user_id . ".";
if($user_id == $group["user_id"])
{
    $result = AddUserToGroup($group);
}
response(200,"result",$result);

//response(200,"SERVER",$_SERVER);
//htmlResponse($token);

?>