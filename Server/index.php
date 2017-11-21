<?php 

const ACCESS_ALLOWED = "ACCESS ALLOWED";
const ACCESS_NOT_ALLOWED = "ACCESS NOT ALLOWED";

$uids = array('toto' => 'titi');
$rfids = array('179.81.223.00');

try
{
    $uid = "";
    $pss = "";
    $rfid = 0;

    if (isset($_GET['uid'])) {
        $uid = (($_GET['uid']));
    }
    else
    {
        throw new Exception("uid is required");
    }

    if (isset($_GET['pss'])) {
        $pss = (($_GET['pss']));
    }
    else
    {
        throw new Exception("pss is required");
    }

    if (isset($_GET['rfid'])) {
        $rfid = (($_GET['rfid']));
    }
    else
    {
        throw new Exception("RFID is required");
    }

    if(isset($uids[$uid]) && $uids[$uid] == $pss)
    {
        $done = false;

        foreach ($rfids as $key => $id) {
            if($id === $rfid)
            {
                echo ACCESS_ALLOWED;
                $done = true;
            }
        }

        if(!$done)
        {
            echo ACCESS_NOT_ALLOWED;
        }
    }
    else
    {
        echo ACCESS_NOT_ALLOWED;
    }

    
}
catch(Exception $e)
{
    echo json_encode(array("error" => $e->getMessage())); 
}