import sys

if len(sys.argv) != 2 or sys.argv[1] != "glad.h":
	print("You need to pass glad.h as a parametr.")
	exit(-1)

enum_values = {}
with open(sys.argv[1]) as file:
	for line in file:
		if line.startswith("#define GL_") and "GL_VERSION_" not in line:
			_define_keyword, enum_text, enum_value = line.split(" ")
			enum_value = int(enum_value, 16)
			
			if enum_value not in enum_values:
				enum_values[enum_value] = enum_text
			else:
				enum_values[enum_value] += f" | {enum_text}"

c_code_lines = []
for enum_value, enum_name in enum_values.items():
	c_code_lines.append('{' f' {enum_value}, "{enum_name}" ' '}') 



c_code = f"""const std::unordered_map<GLenum, const char*> kGLEnumIDToErrorText = 
""" """{
\t""" + ",\n\t".join(sorted(c_code_lines, key=lambda x: int(x.split()[1][0:-1]))) + """
};"""

with open("result.txt", "w+") as f:
	f.write(c_code)