"""
PDF Text Extractor
Extracts text from HiTechnic PDF specifications
"""

import sys

def extract_text_from_pdf(pdf_path):
    """Extract text from PDF file"""
    try:
        import PyPDF2
        
        with open(pdf_path, 'rb') as file:
            pdf_reader = PyPDF2.PdfReader(file)
            text = ""
            
            print(f"Extracting text from: {pdf_path}")
            print(f"Total pages: {len(pdf_reader.pages)}\n")
            print("=" * 80)
            
            for page_num, page in enumerate(pdf_reader.pages, 1):
                print(f"\n--- PAGE {page_num} ---\n")
                page_text = page.extract_text()
                text += page_text
                print(page_text)
                print("\n" + "-" * 80)
            
            # Save to text file
            output_path = pdf_path.replace('.pdf', '.txt')
            with open(output_path, 'w', encoding='utf-8') as out_file:
                out_file.write(text)
            
            print(f"\n\nText saved to: {output_path}")
            
    except ImportError:
        print("PyPDF2 not installed. Installing now...")
        import subprocess
        subprocess.check_call([sys.executable, '-m', 'pip', 'install', 'PyPDF2'])
        print("\nPyPDF2 installed! Please run the script again.")
        
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    pdf_file = r"c:\tetrix motor controllers\extras\HiTechnic-Motor-Controller-Specification.pdf"
    
    # Allow command line argument
    if len(sys.argv) > 1:
        pdf_file = sys.argv[1]
    
    extract_text_from_pdf(pdf_file)
