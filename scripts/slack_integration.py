import sys
from dotenv import load_dotenv
import os

def post_to_slack(s):
    try:
        import slack
        load_dotenv()
        print("Sending to slack")
        # print(os.environ)
        # If lost, retrieve the token from here: https://api.slack.com/apps/AKT5B5SJG/oauth
        client = slack.WebClient(token=os.environ["SLACK_API_TOKEN"])
        response = client.chat_postMessage(channel="#bot-logs", text=s)
        return response
    except:
        print("[slack] Unexpected error:", sys.exc_info()[0:2])


if __name__ == "__main__":
    load_dotenv()

    msg = "Hello hackers! :rocket::rocket::rocket:"
    response = post_to_slack(msg)
    print(response)
    assert response["ok"]
    assert response["message"]["text"] == msg
