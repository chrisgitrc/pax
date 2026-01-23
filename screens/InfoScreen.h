void DBG_SET(SCREEN *s,uint64_t value) {
	static uint8_t allocated=0;
	if (allocated == s->height){allocated=0;}
	snprintf(s->print_buffer[allocated],s->length,"%08"PRIu64,value);
	++allocated;
	
	}
void DBG_SET_STR(){}
void DBG_UPDATE(Binary*b, SCREEN *self, VM_CPU*q) {

		}
