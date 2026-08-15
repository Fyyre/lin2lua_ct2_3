#define MAXQUEUELENGTH 256
#define MAXBUFFERSIZE 256

#include <stdio.h>

/* Simple circular FIFO buffer for text that 
** - does not use 'new' (so does not cause problems with nProtect / Thermida)
** - does not fill up the memory if not emptied!
*/

class FIFO
 {
 public:
	 char m_buffer[MAXQUEUELENGTH][MAXBUFFERSIZE];
	 int m_head;
	 int m_tail;
	 bool m_overwrite;

	 FIFO() {
		 clear();
	 }

	 //virtual ~FIFO() {}
		
	 bool isempty(){
		 return m_head == m_tail;
	 }

	 void clear() {
		 m_head = 0;
		 m_tail = 0;
		 m_overwrite = false;
		 //memcpy(m_buffer, '\0', sizeof(m_buffer));
	 }

	 void queuew(TCHAR * in) {
		sprintf(m_buffer[m_head], "%.*ws", MAXBUFFERSIZE-1, in);
		m_head++;
		if (m_head >= MAXQUEUELENGTH) m_head = 0;
		if (m_head == m_tail) {
			m_head++;
			m_overwrite = true;
		}
		if (m_tail >= MAXQUEUELENGTH) m_tail = 0;
	 }

	 char * dequeue() {
		char * out = NULL;
		if (m_head != m_tail) {
	 		out = m_buffer[m_tail];
			m_tail++;
			if (m_tail >= MAXQUEUELENGTH) m_tail = 0;
		}
		return out;
	 }
 };
