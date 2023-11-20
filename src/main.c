#include "session/session.h"

int main() {
    session_ptr session = new_session(new_connection());
    start_session(session);
    return 0;
}