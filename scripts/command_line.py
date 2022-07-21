from typing import List, Optional, Tuple


# Return flag arguments, where --flag *args*
# Returns all arguments after flag or None if flag is not in args
def retrieve_flag_args(flag: str, argv: List[str]) -> Optional[List[str]]:

    if flag not in argv:
        return None

    flag_pos: int = argv.index(flag)
    return argv[flag_pos + 1:]


# Read plot axis limits from command line
def parse_axis_limits(argv: List[str]) -> \
        Tuple[Optional[List[float]], Optional[List[float]]]:

    if '--fixed-axis' in argv:
        arg_pos = argv.index('--fixed-axis')

        try:
            plot_axis_lb = [float(argv[arg_pos + 1]), float(argv[arg_pos + 2])]
            plot_axis_ub = [float(argv[arg_pos + 3]), float(argv[arg_pos + 4])]
            return plot_axis_lb, plot_axis_ub
        except IndexError:
            return None, None

    return None, None
