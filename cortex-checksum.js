// ARM Cortex binary checksum tool.  Requires Node.js v0.10+: http://nodejs.org
//
// usage:
//   node cortex-checksum.js < unsignedBinaryFile.bin > signedBinaryFile.bin
//
// purpose:
//   ARM officially requires the sum of the first eight 32-bit words to be
//     zero modulo 2^32 on Cortex processors.  These first 8 words correspond
//     to the initial stack value, reset and other interrupt vectors plus a
//     special checksum value:
//
//   0x00000000:	Initial stack pointer value
//   0x00000004:	Reset vector
//   0x00000008:	NMI handler
//   0x0000000C:	HardFault handler
//   0x00000010:	(reserved)
//   0x00000014:	(reserved)
//   0x00000018:	(reserved)
//   0x0000001C:	Checksum value
//   ...
//   ...            (...other interrupt vectors...)
//
//   we set the value of the checksum word (address 0x1C) such that the sum of
//     these eight words is 0 (mod 2^32.)
//
//   N.B. Not every Cortex chip from every licensee enforces this.  In the
//     author's experience, NXP does, ST and Nordic don't.

var vectors = new Buffer(32);
var counter = 0, fixed = false;

process.stdin.on("data", function(data)
{
	if(counter < 32)
	{
		data.copy(vectors, counter);
		counter += data.length;
	}
	
	if(counter >= 32)
	{
		if(!fixed)
		{
			var checksum = 0;
			for(var i = 0; i < 28; i += 4)
				checksum -= vectors.readInt32LE(i);
			vectors.writeInt32LE(checksum, 28);

			process.stdout.write(vectors);
			process.stdout.write(data.slice(data.length - (counter - 32)));
			fixed = true;
		}
		else
			process.stdout.write(data);
	}
});
