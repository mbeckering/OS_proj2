# OS_proj2

Use:
./master -n <1 to 18>

I chose 18 max consumers (master + producer + 18 = 20). I later realized that bash makes it 21 processes, but it still seems to work, so I didn't change it.

I (embarrassingly) count about 45 hours of work on this project (haven't started the bash script yet). Every element was new to me and took hours of research and troubleshooting and help from the instructor (THANK YOU).

I certainly could have created more functions to re-use code, add flexibility, reduce lines, and neaten everything up. I found out I could have passed shared memory info instead of shmgetting it all in each executable. I could have combined some of the shared info into structs, but I have practically no experience with structs. I decided to first make everything work any way I knew how. There is a lot I want to go back and improve. But it works, it meets the criteria, and frankly I'm out of time to spend. I learned a lot but it was a much larger time investment than I expected.
