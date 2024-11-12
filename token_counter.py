import sys
import tiktoken

def count_tokens(text):
    encoding = tiktoken.encoding_for_model("gpt-4ocode")
    tokens = encoding.encode(text)
    return len(tokens)

if __name__ == "__main__":
    text = sys.stdin.read()
    token_count = count_tokens(text)
    print(token_count)
