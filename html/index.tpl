<!DOCTYPE html>
<html>
	<head>
		<title>
			Esp8266 watermeter
		</title>
		<link rel="stylesheet" type="text/css" href="style.css">
	</head>
	<body>		
		<div id="main">
			<h1 style="text-align: center;">Watermeter</h1>
			<h1 style="text-align: center;"><b>%counter%</b> m<sup>3</sup></h1>	
			<p style="text-align: center;"><a href="/daily.tpl">Daily</a>/<a href="/monthly.tpl">Monthly</a> usage</p>		
			<h4 style="text-align: center;">
			  <br /><strong>New value:</strong> <br />
			  <form action="counterValue.cgi" method="post">
			  <input type="text" name="value" value="0" />
			  <input type="submit" name="connect" value="set" />
			</h4>
			<p style="text-align: center;"><a href="/wifi">Wifi settings</a></p>
		</div>
	</body>
</html>
