<?php
require_once 'kpasinc/vars.inc';
require_once 'kpasinc/curlutility.inc';
require_once 'kpasinc/canvas.inc';
require_once 'kpasinc/utility.inc';
require_once 'kpasinc/kpasapi.inc';

$course_id = $_GET["course_id"];
$result = getGroupCategories($course_id);
KPASAPI_Response(200,"result",$result);
?>