from flask import Flask, render_template, request, jsonify
import mysql.connector as msc
from datetime import datetime
import random
import smtplib
from email.mime.text import MIMEText

conn = msc.connect(
    host="127.0.0.1",
    port="3306",
    user="root",
    password="Dhanu@2003",
    database = "keychain"
)

day = 0
mon = 0
year = 0

cur = conn.cursor()

app = Flask(__name__)

pen = 0
hol = 0
user = ''
temp = []
data_ = []
total = 0

credential_status = True
email = ''
gen_otp = 0

admin_user = ''

access_count = 0



def send_email(subject, body, to_email):
    # Your email credentials
    sender_email = "your_mail_id"
    sender_password = "your_app_key"

    # Email content
    message = MIMEText(body)
    message["Subject"] = subject
    message["From"] = sender_email
    message["To"] = to_email

    # Establish a connection to the SMTP server
    with smtplib.SMTP("smtp.gmail.com", 587) as server:
        server.starttls()
        
        # Login to your email account
        server.login(sender_email, sender_password)
        
        # Send the email
        server.sendmail(sender_email, to_email, message.as_string())




@app.route('/')
def index():
    global pen,hol,user,temp,data_,credential_status,gen_otp,email,admin_user
    pen = 0
    hol = 0
    user = ''
    temp = []
    data_ = []
    total = 0
    credential_status = True
    gen_otp=0
    email=''
    admin_user=''
    return render_template('Student_Login.html')


@app.route('/postdata', methods=['POST'])
def post_data():
    global day, mon, year
    data = request.json  # Parse the incoming JSON data
    print(data)
    print(type(data))
    if not data:
        return jsonify({"error": "No data received"}), 400
    cur.execute(f"insert into log_data values('{data['startt']}','{data['stopt']}','{data['tdur']}','{data['dist']}','{data['sloc']}','{data['stoploc']}','{data['avgspeed']}')")
    # Print the received data (for debugging)
    print(f"Received data: {data}")
    if day==0 and mon==0 and year==0:
        day = datetime.now().date().day
        mon = datetime.now().date().month
        year = datetime.now().date().year
    
    cur.execute("select * from access")
    x = cur.fetchall()
    if x==[]:
        cur.execute(f"insert into access values('1','{day}:{mon}:{year}','0')")
        conn.commit()
    cur.execute("select * from access")
    x = cur.fetchall()
    temp_day = int(x[-1][1].split(":")[0])
    temp_c = int(x[-1][2])
    if int(day)==temp_day:
        temp_c+=1
        cur.execute(f"update access set times='{temp_c}' where date='{x[-1][1]}'")
    else:
        cur.execute(f"insert into access values({int(x[-1][0])+1},'{day}:{mon}:{year}','{1}')")
    conn.commit()

    # Respond with a success message
    print(data)
    return jsonify({"Success": "data received"}), 200

@app.route('/postspeedloc',methods=['POST'])
def speed():
    data = request.json
    print(data)
    print(type(data))
    if not data:
        return jsonify({"error": "No data received"}), 400
    cur.execute(f"update speed set value='{data['speed']}';")
    cur.execute(f"update location set latitude='{data['lat']}';")
    cur.execute(f"update location set longitude='{data['lon']}';")
    conn.commit()
    # Print the received data (for debugging)
    print(f"Received data: {data}")

    # Respond with a success message
    
    return jsonify({"Success": "data received"}), 200

@app.route('/availability')
def availability():
    global user,credential_status

    if(user=='' and credential_status):
        return index()
    cur.execute("select * from location;")
    temp = cur.fetchall()[0]
    return render_template('student_Availability.html',data = temp)

@app.route('/penalty')
def penalty():
    global user,temp,total
    if user=='' and credential_status:
        return index()
    
    cur.execute("select * from log_data")
    temp_log = cur.fetchall()
    return render_template('student_penalty.html',data = [temp_log,len(temp_log)])

@app.route('/extension')
def extension():
    global user
    if(user=='' and credential_status):
        return index()
    cur.execute("select * from access")
    temp_acc = cur.fetchall()
    return render_template('student_Extension.html',data = temp_acc)

@app.route('/holdings')
def holdings():
    cur.execute("select distance from log_data;")
    total_dis = sum(list(map(lambda x:float(x[0]), cur.fetchall())))
    cur.execute("select value from speed;")
    currr_spe = int(cur.fetchall()[0][0])
    return render_template('student_MyHoldings.html',data = [currr_spe, total_dis])

@app.route('/student', methods=['POST'])
def process_form():
    global user,credential_status
    # Access form data using request.form
    name = request.form.get('ID')
    password = request.form.get('pass')
    print(password)
    user = name
    if user=="admin" and password=="123456":
        return holdings()
    else:
        return "invalid"
        

        
@app.route('/forget_password')
def forget_pass():
    return render_template('student_forget_pass.html')

@app.route('/resend_otp')
def resend():
    global gen_otp,email
    gen_otp=random.randint(100000,999999)
    subject = "One time password"
    body = "your one time password for changing password   "+str(gen_otp)
    recipient_email = email
    send_email(subject, body, recipient_email)
    return enter_otp()

@app.route('/get_otp', methods=['POST'])
def get_otp():
    global gen_otp,email,user
    field1 = request.form.get('reg_no')
    field2 = request.form.get('mail')
    cur.execute("select name,email from student")
    for i in cur:
        if i[0]==field1 and i[1]==field2:
            user=field1
            email=field2
            gen_otp=random.randint(100000,999999)
            subject = "One time password"
            body = "your one time password for changing password   "+str(gen_otp)
            recipient_email = email
            send_email(subject, body, recipient_email)
            return enter_otp()
        
    return "User not found"

@app.route('/enter_otp')
def enter_otp():
    return render_template('student_OTP_check.html')
    

@app.route('/check_otp',methods = ['POST'])
def check_otp():
    global gen_otp,email,user
    otp = request.form.get('otp')
    if gen_otp==int(otp) and gen_otp!=0 and user!='':
        gen_otp=0
        return render_template('student_new_pass.html')
    else: return index()

@app.route('/new_student')
def new1():
    return render_template('student_new_pass,html')

@app.route('/change_password', methods=['POST'])
def change():
    global user
    field1 = request.form.get('pass1')
    field2 = request.form.get('pass2')

    if field1!=None and (field1==field2):
        cur.execute("update student set password = '"+str(field1)+"' where name = '"+user+"'")
        conn.commit()
        subject = "Your Password changed successfully"
        body = "your password for library dashboard has been changed successfully \n\n USER NAME = "+user+"\nPassword = "+field1+"\n\n use the above credential to access the webpage"
        recipient_email = email
        send_email(subject, body, recipient_email)
        return index()
    else:
        return new1()

    



if __name__ == '__main__':
    app.run(host='0.0.0.0',debug=True,port=5000)
