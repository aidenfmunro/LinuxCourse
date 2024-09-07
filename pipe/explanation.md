shell             cmd1                cmd2               cmd3
-----             ----                ----               ----
pipe(fds);
fork();
close(fds[0]);    close(fds[1]);
                  dup2(fds[0], 0);
                  pipe(fds);
                  fork();
                  close(fds[0]);      close(fds[1]);
                  dup2(fds[1], 1);    dup2(fds[0],0);
                  exec(...);          pipe(fds);
                                      fork();
                                      close(fds[0]);     etc
                                      dup2(fds[1],1);
                                      exec(...);
