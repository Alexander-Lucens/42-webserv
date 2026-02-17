| Port / Range | Category | Browser Status | Usage & Explanation |
| :--- | :--- | :---: | :--- |
| **8080** | **Dev Standard** | ✅ **Safe** | **The Best Choice.** The standard alternative to port 80. Does not require `sudo`. |
| **8081 - 8089** | **Alternates** | ✅ **Safe** | Perfect fallbacks if 8080 is busy. |
| **3000** | **Frameworks** | ✅ **Safe** | Standard for Node.js, React. |
| **4242** | **Custom** | ✅ **Safe** | Great for custom projects (42 School). |
| **8000** | **Backend** | ✅ **Safe** | Common for Python/PHP dev servers. |
| **1024 - 49151** | **Registered** | ✅ **Safe** | **General User Range.** Safe to use (unless a DB is there). |
| **80 / 443** | **System** | ⚠️ **Root** | Standard HTTP/HTTPS. Requires `sudo`. |
| **5432 / 3306** | **Databases** | ❌ **Blocked** | Postgres/MySQL. Browsers block these (ERR_UNSAFE_PORT). |