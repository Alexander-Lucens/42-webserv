import os
from urllib.parse import parse_qs

query = os.environ.get("QUERY_STRING", "")
params = parse_qs(query)


name = params['name'][0]
if not name or name == " ":
	name = "world"

print(f"<h1>Hello {name}!</h1>")
