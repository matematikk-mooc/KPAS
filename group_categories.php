<?php
require_once 'kpasinc/vars.inc';
require_once 'kpasinc/curlutility.inc';
require_once 'kpasinc/canvas.inc';
require_once 'kpasinc/utility.inc';
require_once 'kpasinc/kpasapi.inc';

$course_id = $_GET["course_id"];
try {
    $result = getGroupCategories($course_id);
    if($result.errors && $result.errors.message) {
        $errorMessage = $result.status . " " . $result.errors.message[0];
        throw new Exception($errorMessage);
    }
KPASAPI_Response(200,"result",$result);
?>