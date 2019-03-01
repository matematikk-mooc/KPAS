<?php
require_once 'kpasinc/vars.inc';
require_once 'kpasinc/curlutility.inc';
require_once 'kpasinc/canvas.inc';
require_once 'kpasinc/utility.inc';
require_once 'kpasinc/kpasapi.inc';

$course_id = $_GET["course_id"];
$result = "";
$status_message = "Success";
try {
    $result = getGroupCategories($course_id);
    if(array_key_exists('errors', $result)) {
        $errorMessage = "Canvas: " . $canvasUser["errors"][0]["message"];
        throw new Exception($errorMessage);
    } 
} catch (Exception $e) {
    $result = $e->getMessage();
    $status_message = "Failure";
}
KPASAPI_Response(200,$status_message,$result);
?>