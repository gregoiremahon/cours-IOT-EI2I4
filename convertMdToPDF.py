import os
from markdown2 import markdown
from weasyprint import HTML

pdf_dir = "./pdf"
if not os.path.exists(pdf_dir):
    os.makedirs(pdf_dir)

course_files_dir = "./fiches_cours"
for filename in os.listdir(course_files_dir):
    if filename.endswith(".md"):
        with open(os.path.join(course_files_dir, filename), 'r') as file:
            markdown_content = file.read()

        html_content = markdown(markdown_content)
        pdf_filename = os.path.join(pdf_dir, filename.replace('.md', '.pdf'))
        HTML(string=html_content).write_pdf(pdf_filename)

print("Création des fiches terminée.")
