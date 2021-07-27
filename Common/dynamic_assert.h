#define NOOP ((void)0)

#define handler_return(code) return code
#define handler_ignore() NOOP

#define delete_args(...)
#define keep_args(...) __VA_ARGS__
#define expand(v) v

#define command_name_ handler_ignore delete_args(
#define command_args_ keep_args(

#define command_name_return handler_return delete_args(
#define command_args_return keep_args(

#define command_name(cmd) command_name_ ## cmd)
#define command_args(cmd) command_args_ ## cmd)

#define command(cmd) expand(command_name(cmd)(command_args(cmd)))

#define assertion1(test, cmd1, cmd2) \
    do { if (!(test)) { command(cmd1); command(cmd2); } } while (0)

int test()
{
    assertion1(1 == 1);
    assertion1(1 == 2, return -8);
}
