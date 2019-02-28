<?php
ini_set('display_errors', 1);
session_start();
require_once 'vendor/autoload.php';
require_once 'kpasinc/dataporten.inc';
require_once 'kpasinc/vars.inc';
require_once 'kpasinc/canvas.inc';
require_once 'kpasinc/utility.inc';

use Kasperrt\OAuth2;

$oauth2 = null;
$initialized = false;

$userInfo = "";
$minegrupper = false;
if(isset($_GET['minegrupper'])) {
    $minegrupper = true;
    $_SESSION["minegrupper"] = true;
}
else if(isset($_SESSION['minegrupper'])){
    $minegrupper = true;
}
else if(isset($_GET['course_id'])) {
    $course_id = $_GET['course_id'];
    $_SESSION["courseId"] = $course_id;
}
else if(!isset($_SESSION["courseId"]))
{
    echo "Mangler course_id parameter.";
    exit; 
}

if(!$offline)
{
    $oauth2 = new OAuth2([
        "client_id"          => $client_id,
        "client_secret"      => $client_secret,
        "redirect_uri"       => $redirect_uri,
        "auth"               => "https://auth.dataporten.no/oauth/authorization",
        "token"              => "https://auth.dataporten.no/oauth/token",
        "response_type" 	 => "code",
        "session"			 => true
    ]);
    if(isset($_GET['code'])) {
        $code = $_GET['code'];
        $state = $_GET['state'];
        mydbg("Code:" . $code);
        mydbg("State:" . $state);
        mydbg("Getting access token...");
        $access_token = $oauth2->get_access_token($state, $code);
        mydbg($access_token);
        $_SESSION["token"] = $access_token;	
        $userInfo = getUserInfoFromDataporten($access_token, $oauth2);
        $groupsInfo = getGroupsInfoFromDataporten($access_token, $oauth2);
        $extraUserInfo = getExtraUserInfoFromDataporten($access_token, $oauth2);

        $initialized = true;
    } else if(isset($_GET['logout'])) {
        session_destroy();
        header('HTTP/1.1 302 Found');
        header('Location: https://auth.dataporten.no/logout');
    }else {
        $oauth2->redirect();
    }
}
else
{
    $userInfo = json_decode($offlineUserInfo,true);
    $groupsInfo = json_decode($offlineGroupsInfo,true);
    $extraUserInfo = json_decode($offlineExtraUserInfo,true);
    $initialized = true;
}

if($initialized)
{
    $_SESSION["userInfo"] = $userInfo;
    $_SESSION["groups"] = $groupsInfo;
    $_SESSION["extraUserInfo"] = $extraUserInfo;
    if($minegrupper)
    {
        mydbg("Minegrupper is true");
    	header('Location: minegrupper.php');
    }
    else
    {
    	header('Location: worker.php');
    }
}
?>
