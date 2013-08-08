<?php
//Memcache Init for LAMP
	//	$memcache=new Memcache;
	//	$memcache->connect('localhost', 11211);
	
//Memcache Init for SAE
		// $memcache=memcache_init();

//Memcache Init for BAE
		// require_once ('BaeMemcache.class.php');
		// $memcache = new BaeMemcache();

//SAE KVDB Init
		// $sae_kvdb = new SaeKV();
		// $sae_kvdb->init();	
		
		
function incrMC($mc,$mckey)
{
	// $MCvalue=$mc->get($mckey);
	// $MCvalue++;
	// $mc->set($mckey,$MCvalue);
	
	//$mc->incr($mckey);
	
	$mc->increment($mckey);
}

function decrMC($mc,$mckey)
{
	$mc->decrement($mckey);
	
	// $MCvalue=$mc->get($mckey);
	// $MCvalue--;
	// if($MCvalue>1)
	// {
		// $mc->set($mckey,$MCvalue);
		
	// }
	// else
	// {
		// $MCvalue=0;
		// $mc->set($mckey,$MCvalue);
	// }
}

?>