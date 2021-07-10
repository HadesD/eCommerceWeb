@extends('layouts.app')
@section('title', ($title ?? 'RinPhone') . ' | ' . config('app.name'))
@push('head.css')
    <link href="{{ mix('/css/app.css') }}" rel="stylesheet" id="app-css" />
@endpush
@section('body.content')
    @php
        $r = ssr('/js/app-server.js')->render();
    @endphp
    {!! empty($r) ? '<div id="app"></div>' : $r !!}
@endsection
@push('body.js')
    <script defer src="{{ mix('/js/app-client.js') }}" id="app-js"></script>
@endpush
