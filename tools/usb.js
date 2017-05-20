var scv = ["std", "cls", "vnd"], die = ["dev", "int", "end"];

require("readline").createInterface(
{
	input: process.stdin,
	output: process.stdout
}).on("line", function(line)
{
	var b = new Buffer(line, "hex");

	var r = ((b[0] & 0x80)? "d>h" : "h>d") + " " + scv[(b[0] >> 5) & 3] + " " + scv[(b[0] >> 5) & 3];

	console.log(r + " bReq=" + b[1].toString(16) + " wVal: " + b.readUInt16LE(2).toString(16) + " wIdx: " + b.readUInt16LE(4).toString(16) + " wLen: " + b.readUInt16LE(6).toString(16));
});
