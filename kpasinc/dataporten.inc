<?php

function getExtraUserInfoFromDataporten($access_token, $oauth2)
{
    $user_info_url = "https://api.dataporten.no/userinfo/v1/userinfo";
    $user_info = $oauth2->get_identity($access_token, $user_info_url);
    mydbg2("getExtraUserInfoFromDataporten",3);
    myvardump2($user_info,3);
    return $user_info;
}
function printDataportenExtraUserInfo($extraUserInfo)
{
    mydbg2("<h2>Extra user info</h2>",3);
    myvardump2($extraUserInfo,3);
}

function getUserInfoFromDataporten($access_token, $oauth2)
{
    $user_url = "https://auth.dataporten.no/userinfo";
    $user = $oauth2->get_identity($access_token, $user_url);
    mydbg2("getUserInfoFromDataporten",3);
    myvardump2($user,3);
    return $user;
}

function printDataportenUserInfo($userInfo)
{
    $userid_sec = $userInfo["user"]["userid_sec"][0];
    $feideid = getFeideIdFromDataportenUserInfo($userInfo);

    mydbg2("<h2>UserInfo</h2>",3);

    myvardump2($userInfo,3);
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
    mydbg2("<h2>Groups in dataporten</h2>",3);
    myvardump2($groupsInfo,3);
    foreach ($groupsInfo as $group)
    {    
        mydbg2("<p>" . $group["id"] . "</p>",3);
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