#include <sstream>
#include <cstdio>
#include "Response.hpp"

std::string get_html_header(const std::string &title)
{
    return std::string("<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n")
        + "<meta charset=\"UTF-8\">\n"
        + "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        + "<title>" + title + " | webserv</title>\n"
        + "<style>\n" + get_inline_css() + "\n</style>\n"
        + "</head>\n<body>\n"
        + "<header class=\"header\">\n"
        + "<button id=\"theme-toggle\" class=\"theme-toggle-btn\">\n"
        + "<span id=\"theme-icon\">\n"
        + "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" class=\"size-4.5\"><path stroke=\"none\" d=\"M0 0h24v24H0z\" fill=\"none\"></path><path d=\"M12 12m-9 0a9 9 0 1 0 18 0a9 9 0 1 0 -18 0\"></path><path d=\"M12 3l0 18\"></path><path d=\"M12 9l4.65 -4.65\"></path><path d=\"M12 14.3l7.37 -7.37\"></path><path d=\"M12 19.6l8.85 -8.85\"></path></svg>\n"
        + "</span>\n"
        + "</button>\n"
        + "</header>\n";
}

std::string get_html_footer()
{
    return std::string("<footer>\n")
        + "<p>&copy; 2026 webserv | Developed by Aleksandr Kuzmin | Antoine Margoloff | Lara Krämer</p>\n"
        + "</footer>\n"
        + "<script src=\"/statics/script.js\"></script>\n"
        + "</body>\n</html>";
}

std::string get_inline_css()
{
    return std::string(
        ":root { "
        "--bg-color: #f7f7f9; "
        "--text-main: #1a1a1a; "
        "--text-secondary: #5a5a5a; "
        "--border-color: #e1e1e1; "
        "--btn-bg: rgba(0, 0, 0, 0.03); "
        "--btn-border: rgba(0, 0, 0, 0.1); "
        "--btn-hover-border: rgba(0, 0, 0, 0.3); "
        "--gradient-1: #333333; "
        "--gradient-2: #666666; "
        "--danger-color: #ff3b30; "
        "} "
        "[data-theme=\"dark\"] { "
        "--bg-color: #0a0a0c; "
        "--text-main: #fcfcfc; "
        "--text-secondary: #a1a1aa; "
        "--border-color: #27272a; "
        "--btn-bg: rgba(255, 255, 255, 0.03); "
        "--btn-border: rgba(255, 255, 255, 0.1); "
        "--btn-hover-border: rgba(255, 255, 255, 0.3); "
        "--gradient-1: #ffffff; "
        "--gradient-2: #888888; "
        "--danger-color: #ff453a; "
        "} "
        "body { "
        "font-family: 'Inter', -apple-system, BlinkMacSystemFont, \"Segoe UI\", Roboto, Helvetica, Arial, sans-serif; "
        "margin: 0; padding: 0; background-color: var(--bg-color); color: var(--text-main); "
        "display: flex; flex-direction: column; min-height: 100vh; "
        "transition: background-color 0.3s ease, color 0.3s ease; "
        "} "
        ".header { display: flex; justify-content: flex-end; padding: 1rem; } "
        ".theme-toggle-btn { "
        "background: var(--btn-bg); border: 1px solid var(--btn-border); border-radius: 50%; "
        "width: 35px; height: 35px; font-size: 1.2rem; cursor: pointer; display: flex; "
        "align-items: center; justify-content: center; transition: transform 0.2s ease, border-color 0.2s ease; "
        "color: var(--text-main); "
        "} "
        ".theme-toggle-btn:hover { transform: scale(1.05); border-color: var(--btn-hover-border); } "
        ".container { flex: 1; display: flex; flex-direction: column; align-items: center; justify-content: center; padding: 1rem; text-align: center; } "
        ".title-group { margin-bottom: 40px; min-height: 6rem; } "
        ".title { font-size: 3rem; font-weight: 800; margin: 0 0 20px 0; letter-spacing: -0.05rem; "
        "background: linear-gradient(135deg, var(--gradient-1), var(--gradient-2)); "
        "-webkit-background-clip: text; background-clip: text; color: transparent; } "
        ".subtitle { font-size: 1.1rem; color: var(--text-secondary); margin-bottom: 50px; font-weight: 400; } "
        ".menu-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); gap: 16px; width: 100%; max-width: 800px; } "
        "a { text-decoration: none; color: var(--text-main)} "
        ".nav-btn { position: relative; width: 100%; padding: 16px 24px; font-size: 1rem; font-weight: 500; color: var(--text-main); "
        "background: var(--btn-bg); border: 1px solid var(--btn-border); border-radius: 2rem; "
        "backdrop-filter: blur(10px); -webkit-backdrop-filter: blur(10px); cursor: pointer; overflow: hidden; "
        "transition: transform 0.2s ease, border-color 0.2s ease, box-shadow 0.2s ease; margin-bottom: 10px; } "
        ".nav-btn:hover { transform: translateY(-2px); border-color: var(--btn-hover-border); box-shadow: 0 4px 20px rgba(0, 0, 0, 0.05); } "
        ".nav-btn.danger { color: var(--danger-color); } "
        ".nav-btn.danger:hover { border-color: var(--danger-color); } "
        "footer { padding: 30px 20px; display: flex; flex-direction: row; justify-content: center; border-top: 1px solid var(--border-color); color: var(--text-secondary); font-size: 0.9rem; } "
        ".cards-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 24px; width: 100%; max-width: 500px; margin-bottom: 20px; } "
        ".card { background: var(--btn-bg); border: 1px solid var(--btn-border); border-radius: 16px; padding: 30px; backdrop-filter: blur(10px); -webkit-backdrop-filter: blur(10px); text-align: left; } "
        ".card h2 { margin: 0 0 15px 0; font-size: 1.5rem; color: var(--text-main); } "
        ".input-label { font-size: 0.9rem; color: var(--text-secondary); margin-bottom: 8px; display: block; font-weight: 500;} "
        ".input-field { width: 100%; box-sizing: border-box; padding: 12px 16px; font-size: 1rem; font-family: inherit; color: var(--text-main); background: rgba(128, 128, 128, 0.05); border: 1px solid var(--border-color); border-radius: 8px; outline: none; transition: border-color 0.2s ease, box-shadow 0.2s ease; margin-bottom: 20px; } "
        ".input-field:focus { border-color: var(--text-secondary); box-shadow: 0 0 0 3px var(--btn-bg); } "
        ".input-field::placeholder { color: var(--text-secondary); opacity: 0.5; } "
        ".header-error { font-size: 8rem; font-weight: 800; margin: 0; line-height: 1; background: linear-gradient(135deg, var(--danger-color), #ff8a00); -webkit-background-clip: text; background-clip: text; color: transparent; } "
        ".body-error { font-size: 2rem; font-weight: 600; color: var(--text-main); margin: 20px 0 10px; }"
    );
}

std::string generate_signin_page()
{
    std::string html = get_html_header("Sign In");
    
    html += std::string(
        "<main class=\"container\">\n"
        "<div class=\"title-group\">\n"
        "<h1 class=\"title\">Sign In</h1>\n"
        "<p class=\"subtitle\">Enter your credentials to access protected resources</p>\n"
        "</div>\n"
        "<div class=\"cards-grid\">\n"
        "<div class=\"card\">\n"
        "<h2>Authentication</h2>\n"
        "<form method=\"POST\" action=\"/submit\" id=\"signin-form\">\n"
        "<label class=\"input-label\" for=\"username\">Username</label>\n"
        "<input type=\"text\" id=\"username\" name=\"username\" class=\"input-field\" placeholder=\"Enter your username\" required>\n"
        "<label class=\"input-label\" for=\"password\">Password</label>\n"
        "<input type=\"password\" id=\"password\" name=\"password\" class=\"input-field\" placeholder=\"Enter your password\" required>\n"
        "<button type=\"submit\" class=\"nav-btn\">Sign In</button>\n"
        "<button type=\"reset\" class=\"nav-btn danger\">Clear</button>\n"
        "</form>\n"
        "</div>\n"
        "</div>\n"
        "</main>\n"
    );
    
    html += get_html_footer();
    return html;
}

std::string generate_success_page(const std::string &title, const std::string &message)
{
    std::string html = get_html_header(title);
    
    html += std::string(
        "<main class=\"container\">\n"
        "<div class=\"title-group\">\n"
        "<h1 class=\"title\">" + title + "</h1>\n"
        "<p class=\"subtitle\">Operation Successful</p>\n"
        "</div>\n"
        "<div class=\"cards-grid\">\n"
        "<div class=\"card\" style=\"text-align: center;\">\n"
        "<h2 style=\"color: #10b981; margin-bottom: 20px;\">✓ Success</h2>\n"
        "<p style=\"color: var(--text-main); margin-bottom: 20px;\">" + message + "</p>\n"
        "<p style=\"color: var(--text-secondary); font-size: 0.9rem;\">Redirecting in 3 seconds...</p>\n"
        "<meta http-equiv=\"refresh\" content=\"3;url=/\" />\n"
        "</div>\n"
        "</div>\n"
        "</main>\n"
    );
    
    html += get_html_footer();
    return html;
}
