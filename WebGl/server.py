import mimetypes
mimetypes.add_type('application/javascript', '.glsl')

from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse

app = FastAPI()


app.mount("/", StaticFiles(directory="static", html = True), name="static")
