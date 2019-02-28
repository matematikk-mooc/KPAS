<?php
session_start();
//require_once 'vendor/autoload.php';
require_once 'kpasinc/dataporten.inc';
require_once 'kpasinc/vars.inc';
require_once 'kpasinc/utility.inc';

printHtmlHeader();
printDataportenHeading();
echo "<h2>Token</h2>";
echo $_SESSION["token"];
$userInfo = $_SESSION["userInfo"];
$dataportenGroups = $_SESSION["groups"];
$extraUserInfo = $_SESSION["extraUserInfo"];
printDataportenUserInfo($userInfo);
printDataportenGroupsInfo($dataportenGroups);
printHtmlFooter();

?>