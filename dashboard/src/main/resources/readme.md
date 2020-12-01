# Safrn Dashboard View

## Folder Structure

    .
    ├── templates           # HTML files
        ├── admin           # HTML files of admin webpages
        ├── fragments       # Thymeleaf fragments files
        └── organization    # HTML files of organization webpages
    ├── static              # CSS files, JavaScript files, and images
        ├── css             # CSS files
        ├── images          # images
        └── js              # Javascript files
    └── README.md

## HTML Files

All html files have similar structures:

``` HTML
<!DOCTYPE HTML>
<html xmlns:th="http://www.thymeleaf.org" xmlns:sec="http://www.thymeleaf.org/thymeleaf-extras-springsecurity4">

<head lang="en" th:include="fragments/header :: header"></head> <!-- import headers from fragments/header.html -->

<head>
    <!-- Local headers. E.g., include CSS or JavaScript files only used for this webpage -->
</head>

<body>
    <div th:replace="fragments/title::nav"></div> <!-- import the title bar from fragments/title.html -->
    <div th:replace="fragments/sidebar::sidebar"></div> <!-- import the admin side bar from  fragments/sidebar.html --> <!-- only  admin webpages have this line-->
    <div th:replace="fragments/orgSidebar::orgSidebar"></div><!-- import the organization side bar from  fragments/orgSidebar.html --> <!-- only  organization webpages have this line-->
    <div th:replace="fragments/userInfoBar::userInfoBar"></div> <!-- import the user info bar from  fragments/userInfoBar.html --> <!-- only  organization webpages have this line-->
    <main role="main">
        <!-- main HTML code -->
    </main>
    <div th:replace="fragments/footer::footer"></div>
</body>
</html>
```

## Thymeleaf Fragments Files
`./fragements` contains all reusable fragments.

`./fragements/footer.html` is the html footer, which contains the copyright information.

`./fragements/header.html` is the global headers, which includes `bootstrap`, `jquery`, and `font-awesome`.

`./fragements/orgSidebar.html` is the sidebar of organization webpages. It contains link tabs for *Profile*, *View Organizations*, *Create New Private Study*, *View Private Studies*, *Logout*.

`./fragements/sidebar.html` is the sidebar of admin webpages. It contains link tabs for *Create Organization*, *Browse Repository*, *Email All Organizations*, *Inactivate an Organization*, *Logout*.

`./fragements/title.html` is the top navigation bar. It has the logo of **Stealth Software Technologies**, the main title **SAFRN Dashboard**, and the logo of **ICPSR**.

`./fragements/userInfoBar.html` is the user information bar. It contains the search bar, the draft count, and the user information.

## CSS Files
`./static/css` contains all CSS files.

`./static/style.css` is the globally used CSS file.

`./static/createSession.css` is the CSS file only for the [/organization/createsession](/organization/createsession) webpage.

`./static/home.css` is the CSS file only for the old webpage, which is deprecated.

`./static/searchSession.css` is the CSS file only for the [/organization/searchsession](/organization/searchsession) webpage.

`./static/viewSession.css` is the CSS file only for the [/organization/viewsession](/organization/viewsession) webpage.

## JavaScript Files
`.static/js` contains all JavaScript files.

`.static/js/script.js` is the globally used JS file.

`.static/js/createSession.js` is the JavaScript file only for the [/organization/createsession](/organization/createsession) webpage.

`.static/js/viewSession.js` is the JavaScript file only for the [/organization/viewsession](/organization/viewsession) webpage.
