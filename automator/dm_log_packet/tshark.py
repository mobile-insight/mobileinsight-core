import subprocess

input_data = "0000 40 01 BF 28 1A EB A0 00 00"
temp_file = "temp.pcap"

proc = subprocess.Popen(['text2pcap', '-l', '148', '-', temp_file], stdin=subprocess.PIPE, stdout=subprocess.PIPE, shell=True)
proc.communicate(input_data);


proc = subprocess.Popen(['tshark',
 '-o', 'uat:user_dlts:\"User 1 (DLT=148)\",\"lte-rrc.pcch\",\"0\",\"\",\"0\",\"\"',
 '-r', temp_file,
 '-V'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, shell=True)

stdout_value, stderr_value = proc.communicate(input_data);

print "stdout_value: " + stdout_value
