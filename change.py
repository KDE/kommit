 
import os

def change(f):
    new_line = """/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

"""
    with open(f, 'r+') as file:
        content = file.read()

        if "SPDX" in content:
            print("Has")
            return
        else:
            print("Dont't has")
        file.seek(0)
        file.write(new_line + content)


for root, dirs, files in os.walk("."):
    for file in files:
        if file.endswith(".cpp") or  file.endswith(".h"):
             print(os.path.join(root, file))
             change(os.path.join(root, file))