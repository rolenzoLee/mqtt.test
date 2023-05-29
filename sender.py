import umail
sender = 'rolenzolee@gmail.com'
password = 'eejhqbxaedgauuqg'
recieverEmail = 'pzps986303@gmail.com'
recieverName = "rolenzo"
def sendCAPTCHA():
    smtp = umail.SMTP("smtp.gmail.com", 465, ssl = True)
    smtp.login(sender, password)    
    smtp.to(recieverEmail)
    smtp.write(f"From: Traceable Package <{sender}>\n")
    smtp.write(f"To: {recieverName} <{recieverEmail}>\n")
    smtp.write("Subject: test\n\n")
    smtp.write("Roses are red.\n")
    smtp.write("Violets are blue.\n")
    smtp.write("...\n")
    smtp.send()
    smtp.quit()
