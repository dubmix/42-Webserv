#include <stdio.h>
#include <string.h>
#include "web.h"

int main(){

	web_print_header();

///////////////////////////////checking input

	int len = 0;
	int num = 0;
	char **fields = NULL;
	char *env = getenv("CONTENT_TYPE");
	int boundary_len = 0;

	if( tec_buf_begins(env, "multipart/form-data; boundary") ){

		char *buffer;
		char *original_buffer;
		char *boundary = env + 30;
		boundary_len = tec_string_length(boundary);
		char *cont_len = getenv("CONTENT_LENGTH");
		int original_len = 0;

		if(cont_len){
			len = tec_string_to_int(cont_len);
			original_len = len;
			len += 1;

			buffer = (char *) malloc(sizeof(char) * len );
			fread(buffer, sizeof(char), len, stdin);
		}
		original_buffer = buffer;

		//first counting how many boundaries
		int next = tec_buf_find_str(buffer, len, boundary);

		while(next != -1){

			buffer += boundary_len + next;
			len -= (boundary_len + next);
			num += 1;
			next = tec_buf_find_str(buffer, len, boundary);
		}

		num *= 4;
		num += 1;
		len = original_len;
		int index = 0;
		fields = (char **) malloc(sizeof(char *) * num );
		memset(fields, 0, sizeof(char *) * num  );
		buffer = original_buffer;
		next = tec_buf_find_str(buffer, len, boundary);
		while(next != -1){
			buffer += next - 4;
			len -= (next - 4);
			*buffer = 0;
			buffer += boundary_len + 4;
			len -= (boundary_len + 4);
			if(tec_buf_begins(buffer, "\r\nContent-Disposition: form-data;") ){
				int n = tec_string_find_char(buffer, '\"');
				buffer += n + 1;
				len -= (n + 1);
				fields[index] = buffer;
				index += 1;
				n = tec_string_find_char(buffer, '\"');
				buffer += n;
				len -= n;
				*buffer = 0;
				buffer += 1;
				len -= 1;
				if(*buffer == ';'){
					n = tec_string_find_char(buffer, '\"');
					buffer += n + 1;
					len -= (n + 1);
					fields[index] = buffer;
					index += 1;
					n = tec_string_find_char(buffer, '\"');
					buffer += n;
					len -= n;
					*buffer = 0;
					buffer += 1;
					len -= 1;
					n = tec_string_find_str(buffer, "\r\n\r\n");
					buffer += n + 4;
					len -= (n + 4);
					fields[index] = fields[index - 2];
					index += 1;
					fields[index] = buffer;

				}else{
					buffer += 4;
					len -= 4;
					fields[index] = buffer;
				}
				index += 1;
			}
			next = tec_buf_find_str(buffer, len, boundary);
		}

	}else{
		printf("No uploads detected\n");
		return 0;
	}

/////////////////////////////////ouputting fields

	int i = 0;
	while(fields[i]){
		printf("%s:<br><b>%s</b><p>", fields[i], fields[i+1]);
		i += 2;
	}
	return 0;

}//main*/

