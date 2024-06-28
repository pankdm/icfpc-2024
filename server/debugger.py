from os import getenv

def initialize_debugger():
    if getenv("DEBUG"):
        import multiprocessing

        if multiprocessing.current_process().pid > 1:
            import debugpy

            # debugpy.listen(("0.0.0.0", 8001))
            # print("Debugger is ready to be attached, press F5", flush=True)
            # debugpy.wait_for_client()
            # print("Visual Studio Code debugger is now attached", flush=True)
