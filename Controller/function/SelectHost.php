<?php
session_start();
$hostid=addslashes($_POST['hostid']);
//setcookie("rc_host_id",$hostid,time()+360000);
$_SESSION["rc_host_id"]=$hostid;
echo $hostid;
?>