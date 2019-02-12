<?php

function getExtraUserInfoFromDataporten($access_token, $oauth2)
{
    $user_info_url = "https://api.dataporten.no/userinfo/v1/userinfo";
    $user_info = $oauth2->get_identity($access_token, $user_info_url);
    mydbg("getExtraUserInfoFromDataporten");
    myvardump($user_info);
    return $user_info;
}
function printDataportenExtraUserInfo($extraUserInfo)
{
    global $verbose;
    if(!$verbose) return;

    echo "<h2>Extra user info</h2>";
    myvardump($extraUserInfo);
}

function getUserInfoFromDataporten($access_token, $oauth2)
{
    $user_url = "https://auth.dataporten.no/userinfo";
    $user = $oauth2->get_identity($access_token, $user_url);
    mydbg("getUserInfoFromDataporten");
    myvardump($user);
    return $user;
}

function printDataportenUserInfo($userInfo)
{
    global $verbose;
    if(!$verbose) return;
    $userid_sec = $userInfo["user"]["userid_sec"][0];
    $feideid = getFeideIdFromDataportenUserInfo($userInfo);

    echo "<h2>UserInfo</h2>";

    myvardump($userInfo);
    echo $feideid;
}

function getFeideIdFromDataportenUserInfo($userInfo)
{
    $userid_sec = $userInfo["user"]["userid_sec"][0];
    $start = strpos($userid_sec, ":") + 1;
    $feideid = substr($userid_sec, $start);
    return $feideid;
}

function printDataportenGroupsInfo($groupsInfo)
{
    global $verbose;
    if(!$verbose) return;
    
    $stuff = <<<EOT
    <h2>Groups in dataporten</h2>
EOT;
    myvardump($groupsInfo);
    echo $stuff;
    foreach ($groupsInfo as $group)
    {    
        
        echo "<p>" . $group["id"] . "</p>";
    }
}
function getGroupsInfoFromDataporten($access_token, $oauth2)
{
    $groups_url = "https://groups-api.dataporten.no/groups/me/groups";
    $groups = $oauth2->get_identity($access_token, $groups_url);
    mydbg("getGroupsInfoFromDataporten");
    myvardump($groups);
    return $groups;
}


?>