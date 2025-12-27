const container = document.getElementById("container");
const goSignup = document.getElementById("goSignup");
const goLogin = document.getElementById("goLogin");
const loginBtn = document.getElementById("loginBtn");
const notice = document.getElementById("notice");

let registeredEmail = "";
let registeredPassword = "";
let isRegistered = false;

// Swap buttons
goSignup.addEventListener("click", ()=>container.classList.add("right-panel-active"));
goLogin.addEventListener("click", ()=>container.classList.remove("right-panel-active"));

// Signup form submit
document.getElementById("signupForm").addEventListener("submit", function(e){
  e.preventDefault();
  const email = this.querySelector('input[type="email"]').value;
  const password = this.querySelector('input[type="password"]').value;
  registeredEmail = email;
  registeredPassword = password;
  isRegistered = true;
  loginBtn.disabled = false;
  notice.style.display = "none";
  alert("Registration successful! Now login using same credentials.");
  container.classList.remove("right-panel-active");

  // Center welcome text after swap
  document.querySelectorAll('.welcome-text').forEach(w=>{
    w.style.top = "50%";
    w.style.transform = "translateY(-50%)";
  });
});

// Login form submit
document.getElementById("loginForm").addEventListener("submit", function(e){
  e.preventDefault();
  const email = this.querySelector('input[type="email"]').value;
  const password = this.querySelector('input[type="password"]').value;

  if(!isRegistered){
    alert("Please register first!");
    return;
  }
  if(email === registeredEmail && password === registeredPassword){
    sessionStorage.setItem("loggedIn","true");
    window.location.href="index.html";
  } else {
    alert("Email or password does not match registered account!");
  }
});
