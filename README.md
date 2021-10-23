# cpu_foo

I couldn't sleep one night so I started writing this.

https://gist.github.com/eparadis/0e5fe0aac7bfe005cab65df5d9e4209d

I couldn't leave well enough alone and over the subsequent few days I wrote a little more and a little more. Now I've got this repo.

It's a simple 8bit CPU emulator. Not many opcodes are implemented. The ones that are were choosen off the top of my head. There's a brain-dead "assembler" written in shell using a pile of `sed` and `xxd`.

I think the biggest lesson of this is that I can write this kind of thing (nearly) in my sleep, so I need to move on to other problems. I've not been able to bridge the "machine to high level language" gap, even thought I've messed with many ideas around FORTH and BASIC. I feel like the authors of _The Elements of Computing Systems_ would be really disappointed in me.

What's next? Memory management? String manipulation? Terminal emulation?
