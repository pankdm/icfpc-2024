PYTHON              := poetry run python
FLASK               := ${PYTHON} -m flask
EXPORT_PYTHONPATH   := export PYTHONPATH="$(shell pwd)";

install:
	bash ./install.sh all

install-prompt:
	bash ./install.sh

repl:
	PYTHONPATH=. poetry run python -i repl.py

start:
	docker compose up

rebuild-docker:
	docker compose build

start-server:
	${EXPORT_PYTHONPATH} ${FLASK} --app server/server --debug run --host=0.0.0.0 --port=8000

start-ui:
	cd ui; pnpm i && pnpm dev
