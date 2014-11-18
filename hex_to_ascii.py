import string
import sys

printable_dict = dict(map(lambda(c): (c, c), string.printable))
printable_dict.update({"\x0c" : "."})
printable_dict.update({"\x0d" : "."})
printable_dict.update({"\n" : "<NL>"})
printable_dict.update({"\r" : "<CR>"})
printable_dict.update({"\t" : "<TB>"})

def hex_to_string(line):
    line = line.replace(" ", "") #get rid of spaces
    line = line.replace("\n", "")
    #print line
    return line.decode("hex")
    #return line

def static_var(varname, value):
    def decorate(func):
        setattr(func, varname, value)
        return func
    return decorate

@static_var("printset", set(string.printable))
def str_to_printable(s):
    #return "".join(c if c in str_to_printable.printset else "." for c in s)
    return "".join(printable_dict[c] if c in printable_dict else "." for c in s)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print "not correct number of arguments"
        print "should have python file, input log, output file"
        sys.exit(0)
    log_name = sys.argv[1]
    out_name = sys.argv[2]
    
    log = open(log_name, "r")
    out = open(out_name, "w")
    for line in log:
        line = line.rstrip()
        out.write(line + '\n')
        if not line.startswith("<") and not line.startswith(">") and not line.startswith("#"):
            line = hex_to_string(line)
            out.write(str_to_printable(line) + '\n\n')
    
