import os
from urllib.parse import parse_qs

query = os.environ.get("QUERY_STRING", "")
params = parse_qs(query)

name = params.get("NAME", ["random person"])[0]

print("Content-Type: text/plain")
print()  # blank line

# What's actually returned to the client
print(f"Hello {name}!")