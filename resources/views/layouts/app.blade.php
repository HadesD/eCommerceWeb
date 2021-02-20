<!DOCTYPE html>
<html lang="{{ str_replace('_', '-', app()->getLocale()) }}">
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <meta name="csrf-token" content="{{ csrf_token() }}">
        <title>@yield('title')</title>
        <link rel="icon" href="{{ asset('favicon.ico') }}">
        @stack('head.css')
        <link href="{{ asset('css/dashboard/app.css') }}?v=1.0.0" rel="stylesheet">
    </head>
    <body>
        @yield('body.content')
        @stack('body.js')
    </body>
</html>