@extends('layouts.app')
@section('title', ($title ?? 'RinPhone') . ' | ' . config('app.name'))
@push('head.css')
    <link href="{{ mix('/css/app.css') }}" rel="stylesheet" id="app-css" />
@endpush
@section('body.content')
    @php
        $fallback = '<div id="app"></div>';
        $r = ssr('/js/app-server.js')->fallback($fallback)->render();
    @endphp
    {!! empty($r) ? $fallback : $r !!}
@endsection
@push('body.js')
    <script defer src="{{ mix('/js/app-client.js') }}" id="app-js"></script>
@endpush
