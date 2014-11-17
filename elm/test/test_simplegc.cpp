#include <elm/util/test.h>
#include <elm/alloc/SimpleGC.h>
#include <elm/sys/System.h>

using namespace elm;

const int ACTION_COUNT = 1000;
const int MAX_SIZE = 1024;
const int MAX_BLOCKS = 512;
const int MAX_REDUNDANTS = 256;

static struct {
	char *base;
	int size;
} blocks[MAX_BLOCKS];
static int count = 0;
static int allocated_size = 0;

class MyGC: public SimpleGC {
public:
	MyGC(void): SimpleGC(2048) { }
protected:
	virtual void collect(void) {
		cerr << "DEBUG: collecting!\n";

		// basic collection
		for(int i = 0; i < count; i++)
			mark(blocks[i].base, blocks[i].size);

		// redundant
		if(count)
			for(int i = 0; i < MAX_REDUNDANTS; i++) {
				int j = sys::System::random(count);
				mark(blocks[j].base, blocks[j].size);
			}
	}

};

TEST_BEGIN(simplegc)
	MyGC gc;
	bool success = true;
	for(int i = 0; success && i < ACTION_COUNT; i++) {
		int a = sys::System::random(100);

		// allocation
		if((!count || a < 50) && count < MAX_BLOCKS) {

			// perform allocation
			blocks[count].size = sys::System::random(MAX_SIZE);
			allocated_size += blocks[count].size;
			blocks[count].base = static_cast<char *>(gc.allocate(blocks[count].size));
			cerr << "DEBUG: allocate(0x" << io::hex(blocks[count].size) << ") = "
				 << static_cast<void *>(blocks[count].base) << "-" << static_cast<void *>(blocks[count].base + blocks[count].size - 1)
				 << " [" << io::hex(allocated_size) << "]\n";

			// check overlapping
			for(int i = 0; i < count; i++)
				if((blocks[i].base <= blocks[count].base && blocks[count].base < blocks[i].base + blocks[i].size)
				|| (blocks[i].base <= blocks[count].base + blocks[count].size - 1 && blocks[count].base  + blocks[count].size - 1 < blocks[i].base + blocks[i].size)) {
					cerr << "DEBUG: conflict with " << static_cast<void *>(blocks[i].base) << "-" << static_cast<void *>(blocks[i].base + blocks[i].size - 1) << io::endl;
					success = false;
					break;
				}

			// increment count
			count++;
		}

		// free
		else {
			int p = sys::System::random(count);
			allocated_size -= blocks[p].size;
			cerr << "DEBUG: free(0x" << static_cast<void *>(blocks[p].base) << ":" << io::hex(blocks[p].size)
				 << " [" << io::hex(allocated_size) << "]\n";
			count--;
			blocks[p] = blocks[count];
		}

	}

	CHECK_MSG("long run", success);
TEST_END
