<?php
require_once 'kpasinc/vars.inc';
require_once 'kpasinc/curlutility.inc';
require_once 'kpasinc/canvas.inc';
require_once 'kpasinc/utility.inc';

$course_id = $_GET["course_id"];

function response($status,$status_message,$data)
{
	header("HTTP/1.1 ".$status);
	
	$response['status']=$status;
	$response['status_message']=$status_message;
	$response['data']=$data;
	
	$json_response = json_encode($response);
	echo $json_response;
}
$result = getGroupCategories($course_id);
response(200,"result",$result);
?>