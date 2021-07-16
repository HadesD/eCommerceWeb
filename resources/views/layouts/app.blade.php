<!DOCTYPE html>
<html lang="{{ str_replace('_', '-', app()->getLocale()) }}">
    <head>
        <meta charset="utf-8" />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
        <meta name="description" content="RinPhone.vn - Chuyên đồ chơi công nghệ di động, Apple, Android, Tablet,..." />
        <meta name="csrf-token" content="{{ csrf_token() }}" />
        <title>@yield('title')</title>
        <link rel="icon" href="{{ asset('favicon.ico') }}" />
        @stack('head.css')
    </head>
    <body>
        @yield('body.content')
        @stack('body.js')
    </body>
</html>
