# KPAS
KompetansePlattform Administrativt System

This is a prototype on a web service for communication between
(Canvas)[https://github.com/instructure/canvas-lms] and (Dataporten)[https://dashboard.dataporten.no].

It must be installed on a PHP server and configured either using environment variables or copying vars.inc.example to vars.inc and 
setting the variables there. The environment variables are read as follows:

```
    //Dataporten app
    $client_id      = getenv('APPSETTING_DATAPORTEN_CLIENT_ID');
    $client_secret  = getenv('APPSETTING_DATAPORTEN_CLIENT_SECRET');
    $redirect_uri   = getenv('APPSETTING_DATAPORTEN_REDIRECT_URI');

    //Canvas connection
    $site               = getenv('APPSETTING_CANVAS_URI');
    $canvas_access_key  = getenv('APPSETTING_CANVAS_TOKEN');
    $principalRoleType  = getenv('APPSETTING_CANVAS_PRINCIPAL_ROLE_TYPE');
    $account_id         = getenv('APPSETTING_CANVAS_ACCOUNT_ID');

    //Logging
    $verbose_level = getenv('APPSETTING_LOG_LEVEL');
```

## Testing the connection
You can test the connection to dataporten and Canvas by accessing index.php with a course_id parameter.
A working example can be found here:(http://udirditkpasapp.azurewebsites.net/?course_id=286)[http://udirditkpasapp.azurewebsites.net/?course_id=286].

Note that you will not get any information about your Canvas user when accessing index.php, if you are not registered as a user in Canvas.
To register, just login here:
(https://bibsys.instructure.com/login/saml)[https://bibsys.instructure.com/login/saml]

## Accessing KPAS from Canvas
You need to install the MatematikkMOOC design on Canvas. Instructions on how to do that: (https://github.com/matematikk-mooc/frontend)[https://github.com/matematikk-mooc/frontend]

You then need to set the configuration variables in the file (https://github.com/matematikk-mooc/frontend/blob/master/src/js/modules/dataporten.js)[https://github.com/matematikk-mooc/frontend/blob/master/src/js/modules/dataporten.js]:

```
//Production
    let request = ['email','longterm', 'openid', 'profile', 'userid-feide', 'groups', 'gk_kpas'];
    let dataportenCallback = 'https://bibsys.instructure.com/courses/234?dataportenCallback=1';
    let dataportenClientId = '823e54e4-9cb7-438f-b551-d1af9de0c2cd';
    let kpasapiurl = "https://kpas.dataporten-api.no";    
```

If you do not want to use the entire design, you can grab (https://github.com/matematikk-mooc/frontend/blob/master/src/js/modules/dataporten.js)[https://github.com/matematikk-mooc/frontend/blob/master/src/js/modules/dataporten.js] 
and modify it to your needs. 

You also need to include this javascript library: (https://github.com/andreassolberg/jso)[https://github.com/andreassolberg/jso] which is 
one of the OAuth libraries referenced here: (https://docs.feide.no/developer_oauth/code_and_libraries/libraries.html?highlight=jso)[https://docs.feide.no/developer_oauth/code_and_libraries/libraries.html?highlight=jso].



